#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"


ChatLogic::ChatLogic()
{
    //// STUDENT CODE
    ////


    ////
    //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic()
{
    //// STUDENT CODE
    ////
    std::cout << "CHAT LOGIC DESTRUCTOR.\n";

    // delete all nodes
    std::cout << "LEN NODES: " << _nodes.size() << "\n";
    for (std::unique_ptr<GraphNode>& it : _nodes)
    {
      std::cout << "node " << it.get() << " to be deleted\n";
      //delete it.get();
      it.reset();
    }

    // delete all edges
  	// NODES NOW HAVE OWNERSHIP OF EDGES, SO THIS MUST GO
//     std::cout << "LEN EDGES: " << _edges.size() << "\n";
//     for (std::unique_ptr<GraphEdge>& it : _edges)
//     {
//       	std::cout << it.get();
//         //delete *it;
//       	it.reset();
//     }

    // delete chatbot instance
//     if (_chatBot != nullptr) {
//         delete _chatBot;
//     }
    

    ////
    //// EOF STUDENT CODE
};

// std::unique_ptr<ChatLogic> &ChatLogic::operator=(const ChatLogic &source){
//   std::cout << "ASSIGNING content of instance CHATLOGIC \n";
//   if (this == &source) {
//     return *this;
//   }
// }

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    // find all occurences for current node
    auto token = tokens.begin();
    while (true)
    {
        token = std::find_if(token, 
                             tokens.end(), 
                             [&tokenID](const std::pair<std::string, std::string> &pair) { 
                               return pair.first == tokenID;;
                             });
        if (token != tokens.end())
        {
            element.AddToken(token->second); // add new keyword to edge
            token++;                         // increment iterator to next element
        }
        else
        {
            break; // quit infinite while-loop
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    // load file with answer graph elements
    std::ifstream file(filename);

    // check for file availability and process it line by line
    if (file)
    {
        // loop over all lines in the file
        std::string lineStr;
        while (getline(file, lineStr))
        {
            // extract all tokens from current line
            tokenlist tokens;
            while (lineStr.size() > 0)
            {
                // extract next token
                int posTokenFront = lineStr.find("<");
                int posTokenBack = lineStr.find(">");
                if (posTokenFront < 0 || posTokenBack < 0)
                    break; // quit loop if no complete token has been found
                std::string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

                // extract token type and info
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != std::string::npos)
                {
                    std::string tokenType = tokenStr.substr(0, posTokenInfo);
                    std::string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

                    // add token to vector
                    tokens.push_back(std::make_pair(tokenType, tokenInfo));
                }

                // remove token from current line
                lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
            }

            // process tokens for current line
            auto type = std::find_if(tokens.begin(), 
                                     tokens.end(), 
                                     [](const std::pair<std::string, std::string> &pair) { 
                                       return pair.first == "TYPE"; 
                                     });
            if (type != tokens.end())
            {
                // check for id
                auto idToken = std::find_if(tokens.begin(), 
                                            tokens.end(), 
                                            [](const std::pair<std::string, std::string> &pair) { 
                                              return pair.first == "ID"; 
                                            });
                if (idToken != tokens.end())
                {
                    // extract id from token
                    int id = std::stoi(idToken->second);

                    // node-based processing
                    if (type->second == "NODE")
                    {
                        //// STUDENT CODE
                        ////

                        // check if node with this ID exists already
                      	// FROM: https://knowledge.udacity.com/questions/432116
                      	// ADDITIONAL INSIGHT FROM: https://www.geeksforgeeks.org/lambda-expression-in-c/
                        auto newNode = std::find_if(_nodes.begin(), 
                                                    _nodes.end(), 
                                                    [&id](std::unique_ptr<GraphNode> &node) { 
                                                      return node->GetID() == id; 
                                                    });
                      
                        // create new element if ID does not yet exist
                        if (newNode == _nodes.end())
                        {
                          	// FROM: https://knowledge.udacity.com/questions/120851
                            _nodes.emplace_back(std::make_unique<GraphNode>(id));
                            newNode = _nodes.end() - 1; // get iterator to last element

                            // add all answers to current node
                            AddAllTokensToElement("ANSWER", tokens, **newNode);
                        }

                        ////
                        //// EOF STUDENT CODE
                    }

                    // edge-based processing
                    if (type->second == "EDGE")
                    {
                      
                        //// STUDENT CODE
                        ////

                        // find tokens for incoming (parent) and outgoing (child) node
                        auto parentToken = std::find_if(tokens.begin(), 
                                                        tokens.end(), 
                                                        [](const std::pair<std::string, std::string> &pair) { 
                                                          return pair.first == "PARENT"; 
                                                        });
                        auto childToken = std::find_if(tokens.begin(), 
                                                       tokens.end(), 
                                                       [](const std::pair<std::string, std::string> &pair) { 
                                                         return pair.first == "CHILD"; 
                                                       });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                            // get iterator on incoming and outgoing node via ID search
                          	// FROM: https://knowledge.udacity.com/questions/432116
                          	// ADDITIONAL INSIGHT FROM: https://www.geeksforgeeks.org/lambda-expression-in-c/
                            auto parentNode = std::find_if(_nodes.begin(), 
                                                           _nodes.end(), 
                                                           [&parentToken](std::unique_ptr<GraphNode> &node) { 
                                                             return node->GetID() == std::stoi(parentToken->second); 
                                                           });
                            auto childNode  = std::find_if(_nodes.begin(), 
                                                           _nodes.end(), 
                                                           [&childToken] (std::unique_ptr<GraphNode> &node) { 
                                                             return node->GetID() == std::stoi(childToken ->second); 
                                                           });

                            // create new edge
                            std::unique_ptr<GraphEdge> edge = std::make_unique<GraphEdge>(id);
                          	// FROM: https://knowledge.udacity.com/questions/120851
                            edge.get()->SetChildNode((*childNode).get());
                            edge.get()->SetParentNode((*parentNode).get());
                          
                          	// NODES NOW HAVE OWNERSHIP OF EDGES, SO THIS MUST BE REMOVED
                            //_edges.push_back(edge.get());

                            // find all keywords for current node
                            AddAllTokensToElement("KEYWORD", tokens, *edge);

                            // store reference in child node and parent node
                          	// FROM: https://knowledge.udacity.com/questions/120851
                          	// WE ARE PASSING IT THE POINTER ADDRESS OF A UNIQUE POINTER
                          	// IS THIS CONSIDERED GOOD PRACTICE?
                            (*childNode)  -> AddEdgeToParentNode(edge.get());
                          	
                          	// IS A UNIQUE POINTER, SO IT CANNOT BE COPIED -> ONLY MOVED (BUILT AROUND RULE OF 5)
                          	// FROM: 
                            (*parentNode) -> AddEdgeToChildNode(std::move(edge));
                          	if (edge) {
                              std::cout << "EDGE DOES STILL EXIST POINTING TO: " << edge.get() << "\n";
                            }
                        }

                        ////
                        //// EOF STUDENT CODE
                    }
                }
                else
                {
                    std::cout << "Error: ID missing. Line is ignored!" << std::endl;
                }
            }
        } // eof loop over all lines in the file

        file.close();

    } // eof check for file availability
    else
    {
        std::cout << "File could not be opened!" << std::endl;
        return;
    }

    //// STUDENT CODE
    ////

    // identify root node
  	// GRAPH NODE SHOULD NOT BE A UNIQUE POINTER AS THE VECTOR ALREADY CONTAINES UNIQUE POINTERS
  	// IT WOULD VIOLATE THE PRINCIPLE OF UNIQUE POINTER OWNERSHIP
    GraphNode *rootNode = nullptr;
    for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    {
        // search for nodes which have no incoming edges
        if ((*it)->GetNumberOfParents() == 0)
        {

            if (rootNode == nullptr)
            {
              	// FROM: https://knowledge.udacity.com/questions/129814
                rootNode = (*it).get(); // assign current node to root
            }
            else
            {
                std::cout << "ERROR : Multiple root nodes detected" << std::endl;
            }
        }
    }
	
  	// ADD CHaT BOT HERE
  	
  	// create instance of chatbot using UNIQUE POINTER
    std::unique_ptr<ChatBot> chatBot;
  	chatBot = std::make_unique<ChatBot>("../images/chatbot.png");
  	chatBot.get()->SetChatLogicHandle(this); //IMPORTANT ADDITION SINCE OTHERWISE WE HIT A SEG.FAULT.
  
  	// PASS POINTER (BUT NOT OWNERSHIP) OF CHATBOT TO CHATLOCI
  	SetChatbotHandle(chatBot.get());
  
    // add chatbot to graph root node
    chatBot.get()->SetRootNode(rootNode);
    rootNode->MoveChatbotHere(std::move(chatBot));
    std::cout << "SUCCESFULLY MOVED BOT";
  
    ////
    //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}
